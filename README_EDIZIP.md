# Edizip
By Lyfe

Python3 module / script for generating and extracting .edz style archives.

```
usage: edizip.py [-h] [-o OUTPUT] [-d] [-v] [-m MAGIC] [-x,] target

positional arguments:
  target                Target dir to archive

optional arguments:
  -h, --help            show this help message and exit
  -o OUTPUT, --output OUTPUT
                        Edizip output location, defaults to
                        `target/target.edz` if not set
  -d, --decompress      Decompress file, target becomes archive and output
                        becomes the target output directory. Will decompress
                        in parent dir of archive if output dir not specified.
  -v, --verify          Verifies file magic is accurate, pass an integer
                        representation of your magic to --magic if checking a
                        file with non-standard magic
  -m MAGIC, --magic MAGIC
                        Magic to use when generating or checking headers,
                        defaults to b'EDZN'
  -x, --header          Print file header, will fail if magic does not match.
```