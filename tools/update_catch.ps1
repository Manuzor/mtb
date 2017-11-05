$Stopwatch = [System.Diagnostics.Stopwatch]::StartNew()

$RepoRoot = Get-Item (Join-Path $PSScriptRoot "..")
$CatchReleasesURL = "https://api.github.com/repos/catchorg/Catch2/releases"
$CatchDestinationFilePath = Join-Path $RepoRoot "tests/catch.hpp"

# Find the latest release
$AllReleases = (Invoke-WebRequest $CatchReleasesURL -UseBasicParsing).Content | ConvertFrom-Json
$LatestRelease = $AllReleases[0]
$LatestAssets = $LatestRelease.assets
$LatestCatchHeaderAssets = $LatestAssets | where { $_.name -eq "catch.hpp" }
$HeaderDownloadURL = $LatestCatchHeaderAssets[0].browser_download_url

# Download the file.
Write-Host "Downloading: $HeaderDownloadURL => $CatchDestinationFilePath"
Write-Progress "Updating Catch" "Downloading: $HeaderDownloadURL => $CatchDestinationFilePath"
(New-Object System.Net.WebClient).DownloadFile($HeaderDownloadURL, $CatchDestinationFilePath)

Write-Host ("Updated in {0:N2}s." -f $Stopwatch.Elapsed.TotalSeconds)
