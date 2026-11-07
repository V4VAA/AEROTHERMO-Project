[CmdletBinding()]
param(
    [ValidateSet('AE4080', 'HW1', 'All')]
    [string]$Target = 'AE4080',
    [ValidateSet('Release', 'Debug')]
    [string]$Configuration = 'Release',
    [switch]$Run
)

$ErrorActionPreference = 'Stop'
if ($Run -and $Target -eq 'All') {
    throw 'Choose AE4080 or HW1 when using -Run.'
}

# Find CMake on PATH, or use the copy installed with Visual Studio Build Tools.
$cmakeCommand = Get-Command cmake -ErrorAction SilentlyContinue
if ($cmakeCommand) {
    $cmakePath = $cmakeCommand.Source
} else {
    $vswherePath = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (-not (Test-Path -LiteralPath $vswherePath)) {
        throw 'Install Visual Studio 2022 C++ Build Tools with CMake tools, or add CMake to PATH.'
    }
    $vsInstallPath = & $vswherePath -latest -version '[17.0,18.0)' -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (-not $vsInstallPath) { throw 'Visual Studio 2022 C++ Build Tools were not found.' }
    $cmakePath = Join-Path $vsInstallPath 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
    if (-not (Test-Path -LiteralPath $cmakePath)) { throw 'Install CMake tools in Visual Studio Installer, or add CMake to PATH.' }
}

Push-Location $PSScriptRoot
try {
    & $cmakePath --preset msvc
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

    $buildArguments = @('--build', '--preset', $Configuration.ToLowerInvariant(), '--parallel', '1')
    if ($Target -ne 'All') { $buildArguments += @('--target', $Target) }
    # These small targets need no persistent MSBuild worker pool.
    $buildArguments += @('--', '/nr:false')
    & $cmakePath @buildArguments
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

    if ($Run) {
        # EquilFlow reads GasModels/ and Species/ relative to the working directory.
        & (Join-Path $PSScriptRoot "build\cmake\bin\$Configuration\$Target.exe")
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    }
} finally {
    Pop-Location
}
