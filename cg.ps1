$found_python = 0

if (!$found_python) {
    $path = Invoke-Expression "cmd /c where python"
    if ($LASTEXITCODE -eq 0) {
        $found_python = 1
    }
}

if (!$found_python) {
    $path = Invoke-Expression "cmd /c where python2"
    if ($LASTEXITCODE -eq 0) {
        $found_python = 1
    }
}

if (!$found_python) {
    if ([System.Io.File]::Exists("C:\\python27\\python.exe")) {
        $path = "C:\\python27\\python.exe"
        $found_python = 1
    }
}

if ($found_python) {
    $arguments = $args -join ' '
    Invoke-Expression "& `"$path`" cg-build.py $arguments"
} else {
    Write-Host "Failed to find python.exe. Is it installed and on the PATH?"
}