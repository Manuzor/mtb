param(
  [string[]]$MtbHeaders,
  [string]$OutFilePath = "-",
  [string]$CodeDir
)

$AllHeaders = @(
  "mtb_platform.h";
  "mtb_common.h";
  "mtb_assert.h";
  "mtb_memory.h";
  "mtb_conv.h";
  "mtb_color.h";
)

$RepoRoot = Join-Path -Resolve $PSScriptRoot ".."

$SeparatorTemplate = @"

// ==========================================
// {0}
// ==========================================

"@

function Get-MergedSourceContent([string[]]$FullHeaderFilePaths, [string[]]$IncludedFiles)
{
  foreach($CurrentHeaderFilePath in $FullHeaderFilePaths)
  {
    if((Test-Path $CurrentHeaderFilePath -PathType Leaf) -and ($CurrentHeaderFilePath -notin $IncludedFiles))
    {
      $CurrentLineNumber = 0
      foreach($_ in Get-Content $CurrentHeaderFilePath)
      {
        $CurrentLineNumber++
        $SourceLine = $_.TrimEnd()
        $Line = $SourceLine.TrimStart()
        if($Line -and $Line[0] -eq '#')
        {
          $Line = $Line.Substring(1).TrimStart()
          if($Line.StartsWith("include"))
          {
            $Line = $Line.Substring("include".Length).TrimStart()
            $IsLocalInclude = $Line[0] -eq '"'
            if($IsLocalInclude)
            {
              $IncludedFileName = $Line.Substring(1, $Line.Length-2)
              if($IncludedFileName -in $AllHeaders)
              {
                $IncludedFilePath = Join-Path -Resolve (Split-Path $CurrentHeaderFilePath) $IncludedFileName
                if($IncludedFilePath -notin $IncludedFiles)
                {
                  $Separator = $SeparatorTemplate -f @($IncludedFileName)

                  # Yield
                  $Separator
                  Get-MergedSourceContent $IncludedFilePath $IncludedFiles

                  # Record inclusion
                  $IncludedFiles += $IncludedFilePath
                }

                # Skip the include statement in any case.
                continue
              }
            }
            else
            {
              $IsGlobalInclude = $Line[0] -eq '<'
              if(!$IsGlobalInclude)
              {
                Throw "${CurrentHeaderFilePath}($CurrentLineNumber): Unrecognized #include-statment: $_"
              }
            }
          }
        }

        # Yield
        $SourceLine
      }
    }
  }
}

function Process-HeaderFilePaths([string]$BasePath, [string[]]$Paths)
{
  foreach($Path in $Paths)
  {
    $Result = Resolve-Path $Path -ErrorAction Ignore
    if(!$Result)
    {
      $Result = Join-Path -Resolve $BasePath $Path -ErrorAction Ignore
    }
    if(!$Result)
    {
      Throw "Unknown header file: $Path"
    }

    $Result
  }
}


#
# Main
#

if(!$MtbHeaders)
{
  $MtbHeaders = $AllHeaders
}

if(!$CodeDir)
{
  $CodeDir = Join-Path -Resolve $RepoRoot "code"
}

$ProcessedHeaders = Process-HeaderFilePaths $CodeDir $MtbHeaders
$Content = Get-MergedSourceContent $ProcessedHeaders @()

if($OutFilePath -eq "-")
{
  $Content
}
else
{
  Set-Content -Path $OutFilePath -Value $Content
}
