# BUILD AND RUN INSTRUCTIONS

## You have 3 options to run this project:

### Option 1: Build with Visual Studio (EASIEST)

1. Open file: `d:\Project DS Final\build\WeatherApp.sln` in Visual Studio
2. Right-click on `server` project → Set as Startup Project
3. Click Build → Build Solution (or press F7)
4. Click Debug → Start Without Debugging (or press Ctrl+F5)
5. Server will start automatically
6. Open browser: `http://localhost:3000/login.html`

### Option 2: Build with MSBuild (Command Line)

```powershell
cd "d:\Project DS Final\build"

# Build the project
msbuild server.vcxproj /p:Configuration=Release

# Run the server
.\Release\server.exe
```

Or if MSBuild is not in PATH:

```powershell
# Find Visual Studio MSBuild
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" server.vcxproj /p:Configuration=Release

# Then run
.\Release\server.exe
```

### Option 3: Use Pre-built Version (if available)

Check if `server.exe` already exists:

```powershell
dir build\Release\server.exe
dir build\Debug\server.exe
```

If found, run it directly:

```powershell
.\build\Release\server.exe
# OR
.\build\Debug\server.exe
```

## After Building/Running:

1. Server console will show:

   ```
   === Restaurant Finder API Server ===
   Database initialized with 15 restaurants
   Server starting on port 3000...
   ```

2. Open browser: `http://localhost:3000/login.html`

3. Create account or login to access the system

## Troubleshooting:

**If build fails:**

- Ensure you have Visual Studio 2019/2022 with C++ Desktop Development installed
- Check that cURL library is installed
- Verify Asio library is downloaded (should be in `_deps` folder)

**If server.exe not found:**

- Check `build\Release\server.exe`
- Check `build\Debug\server.exe`
- Build the project first using one of the methods above

**Port 3000 already in use:**

- Close other applications using port 3000
- Or modify port in `src\main.cpp` (line: `app.port(3000)`)
