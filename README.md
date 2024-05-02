Caustic's Parser -- uses grammar specification to compile Caustic source code
into a [CST (Caustic AST)](https://codeberg.org/Caustic/CausticAST)

Uses [ParGlare](https://github.com/igordejanovic/parglare) for parsing,
meant to work with [CausticGrammar](https://codeberg.org/Caustic/CausticGrammar)
to create a [CST (Caustic AST)](https://codeberg.org/Caustic/CausticAST)


# CLI usage

See `cap --help`


# Module usage

```python3
import sys
import parglare
from pprint import pprint
from pathlib import Path

from caustic import parser

grammar = parglare.Grammar.from_file(parser.builtin_grammar('canonical/canonical.pg'))
#                                           ^ builtin_grammar fetches the builtin grammar path from caustic.grammar,
#                                             optionally adding a subdirectory and checking if it exists


parser_ = parglare.GLRParser(grammar, lexical_disambiguation=True,
                             actions=parser.actions.action.all)
#                                           ^ the actions module supplies some generally useful actions,
#                                             as well as actions specifically for creating CST nodes

try:
    forest = parser_.parse(input())
except parglare.ParseError as e:
    # custom error formatting
    print(parser.errors.format_exc(e), file=sys.stderr)

cst = parser_.call_actions(forest.get_first_tree())
pprint(cst)
```
