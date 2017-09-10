try :
    from _pytwogeom import *
except ImportError as e :
    import os
    ld_library_path = open(os.path.join(os.path.dirname(__file__), 'ld_library_path')).read().strip()
    raise ImportError(
        str(e) +' (library path was `%s` at compile time)' % ld_library_path
    )

from subdivision import *
