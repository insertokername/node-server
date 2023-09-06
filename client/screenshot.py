import ctypes

def screenshot(width, height) -> str:
    dll_path = r"C:\Users\ferenti\Documents\actual-projects\node-server\dll"
    ctypes.windll.kernel32.SetDllDirectoryW(dll_path)

    # Load the shared library
    mylibrary = ctypes.CDLL("screenshot.dll")

    # Define the function prototype
    mylibrary.get_screenshot.restype = ctypes.c_char_p

    # Call the C function and get the returned pointer
    c_string_pointer = mylibrary.get_screenshot(width,height)

    # Convert the C string pointer to a Python string
    python_string = ctypes.string_at(c_string_pointer).decode('utf-8')

    # Print the result
    return(python_string)

#if __name__ == "__main__":
#    print(screenshot(160,190))