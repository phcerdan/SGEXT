from ._sgext import *

from . import from_to

# Create sggen alias for tree submodule. Use it with: sgext.sggen
import sys as _sys
_sys.modules['sggen'] = _sys.modules['sgext._sgext.tree']
import sggen

# sgext.table_folder is used in thin functions
from pathlib import Path as _Path
tables_folder = str(_Path(__file__).parent.absolute() / "tables")
