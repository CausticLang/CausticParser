Caustic's Parser -- uses grammar specification to compile Caustic source code
into a [CST (Caustic AST)](https://codeberg.org/Caustic/CausticAST)

Uses [ParGlare](https://github.com/igordejanovic/parglare) for parsing,
meant to work with [CausticGrammar](https://codeberg.org/Caustic/CausticGrammar)
to create a [CST (Caustic AST)](https://codeberg.org/Caustic/CausticAST)


# CLI usage

See `cap --help`


# Example module usage

```python3
import sys
import parglare
from pathlib import Path

from caustic.parser import CausticParser
from caustic.parser.error import format_exc

# CausticParser.from_file() loads the grammar using ParGlare,
# then invokes CausticParser.from_grammar(), which creates a ParGlare
# parser instance and uses that to finally create the parser
p = CausticParser.from_file(Path('<your-grammar-file>'))

try:
    print(p.parse(input()))
except parglare.ParseError as e:
    # custom error formatting (optional)
    print(format_exc(e), file=sys.stderr)
```
