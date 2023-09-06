//compilation with mingw: g++ screenshot.cpp -lgdiplus -mwindows -lole32 -mconsole
//compilation to dll !run in cmd not pws!: g++ screenshot.cpp -m64 -mwindows -mconsole -shared -Wl,-soname,screenshot.dll -o screenshot.dll -lole32 -lgdiplus
#include <stdio.h>
#include <windows.h>
#include <gdiplus.h>
//#include <time.h>
#include <iostream>

#include <fstream>

#include <vector>
#include <string>

int GetEncoderClsid(const WCHAR *format, CLSID *pClsid) {
    using namespace Gdiplus;
    UINT  num = 0;
    UINT  size = 0;

    ImageCodecInfo *pImageCodecInfo = NULL;

    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;

    pImageCodecInfo = (ImageCodecInfo *)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;

    GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }
    free(pImageCodecInfo);
    return 0;
}

char Base64Digits[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
/*from https://www.experts-exchange.com/articles/3216/Fast-Base64-Encode-and-Decode.html*/
int ToBase64Fast(std::vector<BYTE> &sourceVect, std::string &out) {
    char *pDst = &out[0];
    int nLenDst = out.size();
    int nLenSrc = sourceVect.size();
    BYTE *pSrc = &sourceVect[0];
    int nLenOut = ((nLenSrc + 2) / 3) * 4; // 4 out for every 3 in, rounded up
    if (nLenOut + 1 > nLenDst) {
        return(0); // fail!
    }
    DWORD *pdwDst = (DWORD *)pDst;
    while (nLenSrc > 2) {
        DWORD n = pSrc[0];  // xxx1 // roll the data in...
        n <<= 8;           // xx1x
        n |= pSrc[1];      // xx12
        n <<= 8;           // x12x
        n |= pSrc[2];      // x123

        BYTE m4 = n & 0x3f;  n >>= 6;  // roll it out, 6 bits at a time
        BYTE m3 = n & 0x3f;  n >>= 6;
        BYTE m2 = n & 0x3f;  n >>= 6;
        BYTE m1 = n & 0x3f;

        n = Base64Digits[m4];  n <<= 8;
        n |= Base64Digits[m3];  n <<= 8;
        n |= Base64Digits[m2];  n <<= 8;
        n |= Base64Digits[m1];

        *pdwDst++ = n;   // write four bytes in one operation
        nLenSrc -= 3;
        pSrc += 3;
    }
    // -------------- end of buffer special handling (see text)
    pDst = (char *)pdwDst;
    if (nLenSrc > 0) {  // some left after last triple
        int n1 = (*pSrc & 0xfc) >> 2;
        int n2 = (*pSrc & 0x03) << 4;
        if (nLenSrc > 1) {  // corrected.  Thanks to jprichey
            pSrc++;
            n2 |= (*pSrc & 0xf0) >> 4;
        }
        *pDst++ = Base64Digits[n1]; // encode at least 2 outputs
        *pDst++ = Base64Digits[n2];
        if (nLenSrc == 2) {  // 2 src bytes left to encode, output xxx=
            int n3 = (*pSrc & 0x0f) << 2;
            pSrc++;
            n3 |= (*pSrc & 0xc0) >> 6;
            *pDst++ = Base64Digits[n3];
        }
        if (nLenSrc == 1) {  // 1 src byte left to encode, output xx==
            *pDst++ = '=';
        }
        *pDst++ = '=';
    }
    // *pDst= 0; nLenOut++ // could terminate with NULL, here
    return(nLenOut);
}

bool SavePngMemory(Gdiplus::Bitmap *gdiBitmap, std::vector<BYTE> &data) {
    //write to IStream
    IStream *istream = nullptr;
    CreateStreamOnHGlobal(NULL, TRUE, &istream);

    CLSID clsid_bmp;
    CLSIDFromString(L"{557cf400-1a04-11d3-9a73-0000f81ef32e}", &clsid_bmp);
    Gdiplus::Status status = gdiBitmap->Save(istream, &clsid_bmp, NULL);
    if (status != Gdiplus::Status::Ok)
        return false;

    //get memory handle associated with istream
    HGLOBAL hg = NULL;
    GetHGlobalFromStream(istream, &hg);

    //copy IStream to buffer
    int bufsize = GlobalSize(hg);
    data.resize(bufsize);

    //lock & unlock memory
    LPVOID pimage = GlobalLock(hg);
    memcpy(&data[0], pimage, bufsize);
    GlobalUnlock(hg);

    istream->Release();
    return true;
}

Gdiplus::Bitmap *ResizeClone(Gdiplus::Bitmap *bmp, INT width, INT height) {
    UINT o_height = bmp->GetHeight();
    UINT o_width = bmp->GetWidth();
    INT n_width = width;
    INT n_height = height;
    double ratio = ((double)o_width) / ((double)o_height);
    if (o_width > o_height) {
        // Resize down by width         
        n_height = static_cast<UINT>(((double)n_width) / ratio);
    }
    else { n_width = static_cast<UINT>(n_height * ratio); }
    Gdiplus::Bitmap *newBitmap = new Gdiplus::Bitmap(n_width, n_height, bmp->GetPixelFormat());
    Gdiplus::Graphics graphics(newBitmap);
    graphics.DrawImage(bmp, 0, 0, n_width, n_height);
    return newBitmap;
}

std::string gdiscreen(int width, int height) {
    std::string output_string;
    using namespace Gdiplus;
    IStream *istream;
    HRESULT res = CreateStreamOnHGlobal(NULL, true, &istream);
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    {
        HDC scrdc, memdc;
        HBITMAP membit;
        scrdc = ::GetDC(0);
        int desktop_Height = GetSystemMetrics(SM_CYSCREEN);
        int desktop_Width = GetSystemMetrics(SM_CXSCREEN);
        int resized_width = width;
        int resized_height = height;
        //int Height = height;
        //int Width = width;


        memdc = CreateCompatibleDC(scrdc);
        membit = CreateCompatibleBitmap(scrdc, desktop_Width, desktop_Height);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(memdc, membit);
        BitBlt(memdc, 0, 0, desktop_Width, desktop_Height, scrdc, 0, 0, SRCCOPY);


        Gdiplus::Bitmap temp_bitmap(membit, NULL);

        Gdiplus::PixelFormat format = temp_bitmap.GetPixelFormat();
        Gdiplus::Bitmap *bitmap = ResizeClone(&temp_bitmap, resized_width, resized_height);
        //Gdiplus::Bitmap bitmap(resized_width, resized_height, format);

        CLSID clsid;
        GetEncoderClsid(L"image/jpeg", &clsid);


        std::vector<BYTE> mem_vect;
        SavePngMemory(bitmap, mem_vect);

        int out_size = ((mem_vect.size() + 2) / 3) * 4 + 3;
        output_string.resize(out_size);
        std::cout << "passed";


        int len = ToBase64Fast(mem_vect, output_string);

        std::cout << "len is: " << len;

        bitmap->Save(L"screen.jpeg", &clsid, NULL); // To save the jpeg to a file
        bitmap->Save(istream, &clsid, NULL);

        // Create a bitmap from the stream and save it to make sure the stream has the image
//		Gdiplus::Bitmap bmp(istream, NULL);
//		bmp.Save(L"t1est.jpeg", &clsid, NULL);             
        // END

        //delete &clsid;
        delete bitmap;
        istream->Release();
        DeleteObject(memdc);
        DeleteObject(membit);
        ::ReleaseDC(0, scrdc);

    }
    GdiplusShutdown(gdiplusToken);
    return(output_string);
}
/*
int main() {
    std::ofstream ofile("out.txt");

    ofile << gdiscreen(1920, 1080);
}
/*
int main() {
    clock_t t1 = clock();
    int i;
    int iterations = 1;
    std::ofstream ofile("out.txt");
    for (i = 0;i < iterations;i++) {
        ofile << gdiscreen(1920, 1080);
    }
    clock_t t2 = clock();
    std::cout << iterations << "iterations: " << iterations / ((double)(t2 - t1) / CLOCKS_PER_SEC) << "fps\n";
    return 0;
}*/
extern "C" {
    const char *get_screenshot(int width, int height) {
        static std::string out = gdiscreen(width, height);
        //std::cout << out.size();
        return out.c_str();
    }
}