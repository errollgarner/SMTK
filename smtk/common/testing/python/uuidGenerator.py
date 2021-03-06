#=============================================================================
#
#  Copyright (c) Kitware, Inc.
#  All rights reserved.
#  See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.  See the above copyright notice for more information.
#
#=============================================================================
import os
import sys
import unittest
import smtk
from smtk import common
import smtk.testing
import uuid


class UUIDGenerator(unittest.TestCase):

    def test(self):
        idgen = smtk.common.UUIDGenerator()
        pythonNullUUID = uuid.UUID('00000000-0000-0000-0000-000000000000')
        self.assertEqual(idgen.null(), pythonNullUUID,
                         'Null UUID not generated by UUIDGenerator')
        self.assertEqual(type(idgen.random()), type(
            pythonNullUUID), 'Random UUID not a UUID.')


if __name__ == '__main__':
    smtk.testing.process_arguments()
    unittest.main()
