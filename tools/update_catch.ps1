$RepoRoot = Get-Item (Join-Path $PSScriptRoot "..")
$CatchReleasesURL = "https://api.github.com/repos/philsquared/Catch/releases"
$CatchDestinationFilePath = Join-Path $RepoRoot "tests/catch.hpp"

# Find the latest tag
$AllReleases = (Invoke-WebRequest $CatchReleasesURL -UseBasicParsing).Content | ConvertFrom-Json
$LatestRelease = $AllReleases[0]
$LatestAssets = $LatestRelease.assets
$LatestCatchHeaderAssets = $AllReleases[0].assets | where { $_.name -eq "catch.hpp" }
$HeaderDownloadURL = $LatestCatchHeaderAssets[0].browser_download_url

# Download the file.
Write-Host "Downloading: $HeaderDownloadURL => $CatchDestinationFilePath"
Write-Progress "Updating Catch" "Downloading: $HeaderDownloadURL => $CatchDestinationFilePath"
$DownloadTime = Measure-Command {
  (New-Object System.Net.WebClient).DownloadFile($HeaderDownloadURL, $CatchDestinationFilePath)
}
Write-Host ("Downloaded in {0:N2}s." -f $DownloadTime.TotalSeconds)
