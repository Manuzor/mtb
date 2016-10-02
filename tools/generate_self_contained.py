import os, sys
import argparse
from pathlib import *

makerRegistry = {}

def maker(name):
  def helper(func):
    makerRegistry[name] = func
    return func
  return helper

def remove_includes(content, *includeNames):
  # TODO: Could be more efficient.
  for name in includeNames:
    content = content.replace('#include "{}"'.format(name), '')
    content = content.replace('#include <{}>'.format(name), '')
  return content


#
# Makers
#
@maker('mtb_platform')
def make_mtb_platform(codeDir):
  platformContent = (codeDir / 'mtb_platform.hpp').read_text()
  return platformContent

@maker('mtb_common')
def make_mtb_common(codeDir):
  platformContent = make_mtb_platform(codeDir)
  mtbContent = (codeDir / 'mtb_common.hpp').read_text()
  mtbContent = remove_includes(mtbContent, 'mtb_platform.hpp')
  return '{}\n\n{}'.format(platformContent, mtbContent)

@maker('mtb_assert')
def make_mtb_assert(codeDir):
  mtbContent = make_mtb_common(codeDir)
  assertContent = (codeDir / 'mtb_assert.hpp').read_text()
  assertContent = remove_includes(assertContent, 'mtb_common.hpp')
  return '{}\n\n{}'.format(mtbContent, assertContent)

@maker('mtb_memory')
def make_mtb_memory(codeDir):
  mtbContent = make_mtb_common(codeDir)
  assertContent = make_mtb_assert(codeDir)
  memoryContent = (codeDir / 'mtb_memory.hpp').read_text()
  memoryContent = remove_includes(memoryContent, 'mtb_common.hpp', 'mtb_assert.hpp')
  return '{}\n\n{}\n\n{}'.format(mtbContent, assertContent, memoryContent)


#
# Dispatch
#
def dispatch(name, codeDir):
  if name == 'all':
    return make_mtb_memory(codeDir)
  else:
    if not name in makerRegistry:
      print("Unknown:", name, file=sys.stderr)
      sys.exit(1)

    make = makerRegistry[name]
    return make(codeDir)


#
# Main
#
if __name__ == '__main__':
  thisFilePath = Path(__file__).resolve()
  repoDir = thisFilePath.parent.parent

  parser = argparse.ArgumentParser()
  parser.add_argument('mtbName',
                      nargs='?',
                      default='all',
                      choices=['all'] + list(makerRegistry.keys()),
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
  name = args.mtbName
  outFileName = args.outfile
  codeDir = args.codeDir.resolve()
  content = dispatch(name, codeDir)

  outFile = sys.stdout
  if str(outFileName) != '-':
    outFile = outFileName.open('w')

  print(content, file=outFile)
