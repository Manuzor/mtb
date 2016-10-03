import os, sys
import argparse
from pathlib import *


allHeaders = [
  'mtb_platform.hpp',
  'mtb_common.hpp',
  'mtb_assert.hpp',
  'mtb_memory.hpp',
  'mtb_slice.hpp',
  'mtb_conv.hpp',
]

separatorTemplate = '''
// ==========================================
// {}
// ==========================================
'''

def get_content(*fullHeaderFilePaths, alreadyIncluded):
  for currentHeaderFilePath in fullHeaderFilePaths:
    if currentHeaderFilePath.exists() and currentHeaderFilePath.is_file():
      if currentHeaderFilePath not in alreadyIncluded:
        alreadyIncluded.append(alreadyIncluded)
        with currentHeaderFilePath.open('r') as file:
          for sourceLine in file:
            sourceLine = sourceLine.rstrip()
            line = sourceLine.lstrip()
            if line and line[0] == '#':
              line = line[1:].lstrip()
              if line.startswith('include'):
                line = line[7:].strip()
                isLocalInclude = line[0] == '"'
                isGlobalInclude = line[0] == '<'
                if isLocalInclude:
                  includedFileName = line[1:-1]
                  if includedFileName in allHeaders:
                    includedFilePath = (currentHeaderFilePath.parent/includedFileName).resolve()
                    if includedFilePath not in alreadyIncluded:
                      alreadyIncluded.append(includedFilePath)
                      separator = separatorTemplate.format(includedFileName)
                      yield separator
                      for innerLine in get_content(includedFilePath, alreadyIncluded=alreadyIncluded):
                        yield innerLine
                    # Skip the include statement in any case.
                    continue
                else:
                  assert isGlobalInclude, "wtf?"
            yield sourceLine


def process_header_file_paths(basePath, paths):
  result = []
  if len(paths) == 0:
    result = [basePath/x for x in allHeaders]
  else:
    for path in headerFilePaths:
      fullPath = None
      if path.exists():
        fullPath = path.resolve()
      elif (basePath/path).exists():
        fullPath = (basePath/path).resolve();
      else:
        print('Unknown header file:', path, file=sys.stderr)
      if fullPath:
        result.append(fullPath)
  return result

#
# Main
#
if __name__ == '__main__':
  thisFilePath = Path(__file__).resolve()
  repoDir = thisFilePath.parent.parent

  parser = argparse.ArgumentParser()
  parser.add_argument('mtb_headers',
                      nargs='*',
                      type=Path,
                      help='The name of the library to create a self-contained version of.')
  parser.add_argument('-o', '--outfile', dest='outfile',
                      type=Path,
                      default=Path('-'),
                      help='The output file.')
  parser.add_argument('--code-dir', dest='codeDir',
                      type=Path,
                      default=(repoDir / "code"),
                      help='The directory of the mtb source code.')

  args = parser.parse_args()
  headerFilePaths = args.mtb_headers
  outFileName = args.outfile
  codeDir = args.codeDir.resolve()

  headerFilePaths = process_header_file_paths(codeDir, headerFilePaths)
  content = get_content(*headerFilePaths, alreadyIncluded=[])

  outFile = sys.stdout
  if str(outFileName) != '-':
    outFile = outFileName.open('w', newline='\n')

  for line in content:
    print(line, file=outFile)
