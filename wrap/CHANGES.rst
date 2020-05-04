Changelog (sgext)
==================

0.9.2 (unreleased)
------------------

- Removed the python type: array3d.

  Automatically convert from python types.
  Before: ::

      sn = sg.graph.spatial_node()
      sn.pos = sg.core.array.array3d(2,4,1)

  Now: ::

      sn = sg.graph.spatial_node()
      sn.pos = [2,4,1]

  The drawback of this approach is that `pos` can only be modified as
  whole. The following is invalid::

      sn.pos[1] = 4.0

  We have to use: ::

      sn.set_pos(0, 4.0)
      # or modify the whole pos at once
      sn.pos = [4.0, sn.pos[1], sn.pos[2]]

0.9.1.2
------------------

- Working python for all platforms:

  - Linux: manylinux-2010
  - Windows (vs2019)
  - macOS (target deployment: 10.9)
