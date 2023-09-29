
# import ctypes
#
#def screenshot(width, height) -> str:
#    dll_path = r"C:\Users\ferenti\Documents\actual-projects\node-server\dll"
#    ctypes.windll.kernel32.SetDllDirectoryW(dll_path)
#
#    # Load the shared library
#    mylibrary = ctypes.CDLL("screenshot.dll")
#
#    # Define the function prototype
#    mylibrary.get_screenshot.restype = ctypes.c_char_p
#
#    # Call the C function and get the returned pointer
#    c_string_pointer = mylibrary.get_screenshot(width,height)
#
#    # Convert the C string pointer to a Python string
#    python_string = ctypes.string_at(c_string_pointer).decode('utf-8')
#
#    # Print the result
#    return(python_string) 


from subprocess import Popen, PIPE

def screenshot(width, height) -> str:
    # Initialize c++ screenshoting subprocess
    process = Popen(['.\\screenshot.exe',str(width), str(height)], stdout=PIPE, stderr=PIPE)
    
    # Wait for screenshot to write to out.txt and print the returned val
    print("returned",process.wait())
    
    if(process.returncode!=0):
       raise Exception("subprocess error")
    
    ## Get output of program
    #stdout = process.communicate()
    #print(stdout.decode('utf-8'))
    
    output_base64_screenshot = ''
    with open('out.txt','r') as input_file:
        output_base64_screenshot = input_file.read() 

    return output_base64_screenshot    
    
if __name__ == "__main__":
    print(screenshot(160,190))