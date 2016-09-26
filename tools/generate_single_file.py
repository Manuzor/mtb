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

def make_glue(mostSignificant, *others):
  template = '#if defined({0}) && !defined({1})\n' +\
             '#define {1}\n' +\
             '#endif\n'
  glue = '\n'.join(template.format(mostSignificant, name) for name in others)
  return glue


#
# Makers
#
@maker('mtb_platform')
def make_mtb_platform(codeDir, *, needsGlue=True):
  platformContent = (codeDir / 'mtb_platform.hpp').read_text()
  # Note: mtb_platform.hpp needs no glue.
  return platformContent

@maker('mtb')
def make_mtb(codeDir, *, needsGlue=True):
  platformContent = make_mtb_platform(codeDir, needsGlue=False)
  mtbContent = (codeDir / 'mtb.hpp').read_text()
  mtbContent = remove_includes(mtbContent, 'mtb_platform.hpp')
  # Note: mtb.hpp needs no glue.
  return '{}\n\n{}'.format(platformContent, mtbContent)

@maker('mtb_assert')
def make_mtb_assert(codeDir, *, needsGlue=True):
  mtbContent = make_mtb(codeDir, needsGlue=False)
  assertContent = (codeDir / 'mtb_assert.hpp').read_text()
  assertContent = remove_includes(assertContent, 'mtb.hpp')
  glue = ''
  if needsGlue:
    glue = make_glue('MTB_ASSERT_IMPLEMENTATION', 'MTB_IMPLEMENTATION')
  return '{}{}\n\n{}'.format(glue, mtbContent, assertContent)

@maker('mtb_memory')
def make_mtb_memory(codeDir, *, needsGlue=True):
  mtbContent = make_mtb(codeDir, needsGlue=False)
  assertContent = make_mtb_assert(codeDir, needsGlue=False)
  memoryContent = (codeDir / 'mtb_memory.hpp').read_text()
  memoryContent = remove_includes(memoryContent, 'mtb.hpp', 'mtb_assert.hpp')
  glue = ''
  if needsGlue:
    glue = make_glue('MTB_ASSERT_IMPLEMENTATION', 'MTB_IMPLEMENTATION', 'MTB_ASSERT_IMPLEMENTATION')
  return '{}{}\n\n{}'.format(glue, mtbContent, assertContent)


#
# Dispatch
#
def dispatch(name, codeDir):
  if name == 'all':
    return make_mtb_memory(codeDir)
  else:
    if not name in makerRegistry:
      print("Unknown:", name, file=sys.error)
      sys.exit(1)

    make = makerRegistry[name]
    return make(codeDir)


#
# Main
#
if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument('mtbName',
                      nargs='?',
                      default='all',
                      help='The name of the library to create a self-contained version of.')
  parser.add_argument('--code-dir', dest='codeDir',
                      type=Path,
                      default=(Path(__file__).parent.parent / "code"),
                      help='The directory of the mtb source code.')

  args = parser.parse_args()
  name = args.mtbName
  codeDir = args.codeDir.resolve()
  content = dispatch(name, codeDir)

  print(content, file=sys.stdout)
