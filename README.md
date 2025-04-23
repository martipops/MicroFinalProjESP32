## Instructions
1. Make sure PlatformIO and Node.js is downloaded
2. Clone the repo
3. Rename `include/secrets.h.example` to `secrets.h` and change the necessary information.
4. Run the setup commands
```
cd web
npm i; npm run build
cd ..
python convert_html.py --nobuild
```
5. Build and Upload the PlatformIO project.
