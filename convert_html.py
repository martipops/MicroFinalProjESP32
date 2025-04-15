import os
import subprocess
import gzip

def convert_html_to_header():
    # Build the vue project
    print("Building the Vue project...")
    subprocess.run("cd microfinal-web; npm run build", shell=True, check=True)

    html_file = "microfinal-web/dist/index.html"
    header_file = "include/index_html.h"
    
    if os.path.exists(html_file):
        print(f"Converting {html_file} to {header_file}...")
        
        # Create directory if it doesn't exist
        os.makedirs(os.path.dirname(header_file), exist_ok=True)
        
        # Compress the HTML file
        with open(html_file, 'rb') as f_in:
            html_content = f_in.read()
        
        # Write compressed data as a PROGMEM array
        with open(header_file, 'w') as f_out:
            f_out.write('#include <pgmspace.h>\n\n')
            f_out.write(f'// Original size: {len(html_content)} bytes\n')
            
            # Compress the data
            compressed_content = gzip.compress(html_content, compresslevel=9)
            f_out.write(f'// Compressed size: {len(compressed_content)} bytes\n\n')
            
            f_out.write(f'const size_t index_html_gz_len = {len(compressed_content)};\n')
            f_out.write('const uint8_t index_html_gz[] PROGMEM = {')
            
            # Write the byte array with line breaks for readability
            for i, byte in enumerate(compressed_content):
                if i % 16 == 0:
                    f_out.write('\n  ')
                f_out.write(f'0x{byte:02x}, ')
            
            f_out.write('\n};\n')
            
        print(f"Conversion complete: {header_file}")
        print(f"Original size: {len(html_content)} bytes")
        print(f"Compressed size: {len(compressed_content)} bytes")
    else:
        print(f"HTML file not found: {html_file}")

# Check if running as main script
if __name__ == "__main__":
    convert_html_to_header()
else:
    # Running as PlatformIO pre-script
    try:
        from SCons.Script import Import
        Import("env")
        env.AddPreAction("buildprog", lambda *args, **kwargs: convert_html_to_header())
    except ImportError:
        print("Warning: SCons environment not found, script may not be running in PlatformIO context")