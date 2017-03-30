from pathlib import *
import argparse
import json

thisDir = Path(__file__).parent.resolve()
repoDir = thisDir.parent

def main(destFilePath):
  content = {}
  content["folders"] = [{ 'path': str(repoDir) }]
  content["build_systems"] = [
    {
      "name": "mtb",
      "file_regex": "([A-z]:.*?)\\(([0-9]+)(?:,\\s*[0-9]+)?\\)",

      "variants":
      [
        {
          "name": "Tests: Build Only",
          "windows":
          {
            "cmd": [ "{}/tests/build.bat".format(repoDir) ],
          },
        },
        {
          "name": "Tests: Build and Run",
          "windows":
          {
            "cmd": [ "{}/tests/build.bat".format(repoDir), "run" ],
          },
        }
      ]
    },
  ]

  if not destFilePath.parent.exists():
    destFilePath.parent.mkdir(parents=True)

  with destFilePath.open('w') as destFile:
    json.dump(content, destFile)

if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument('dest',
                      nargs='?',
                      type=Path,
                      default=(repoDir / '_workspace' / 'mtb.sublime-project'),
                      help='The destination file.')
  args = parser.parse_args()
  main(destFilePath=args.dest)
