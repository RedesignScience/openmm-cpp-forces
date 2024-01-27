Python API
==========

To use the plugin from your Python script, you can do:

.. code-block:: python

    import openmm as mm
    import openmmcppforces

    force = openmmcppforces.CompositeRMSDForce()

This is the implemented subclass of :OpenMM:`Force`:

.. toctree::
    :titlesonly:

    CompositeRMSDForce


.. testsetup::

    from openmmcppforces import *
