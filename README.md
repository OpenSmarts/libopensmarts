# libopensmarts

Utility library for creating and interacting with OpenSmarts compatible devices on embedded linux

By default, OSm devices are either onboard, or networked.  If onboard in an embedded linux environment, they should advertise themselves as a socket under the /run/osm/onboard directory.  If on the network, they are accessable via port 1313.
