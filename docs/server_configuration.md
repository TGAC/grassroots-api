# Server Configuration

The global Server configuration is specified by a JSON file called *grassroots.config" at the top level of the Grassroots directory.
Unlike the individual [Service configuration](service_configuration.md) files, any changes in this file require a restart of the Grassroots Server to take effect.

## Options

There are various keys that can be placed in this file described below.

 * **provider**:
  * **name**:
  * **description**:
  * **uri**: 
 * **servers**:
  * **server_name**:
  * **server_uri**:
  * **paired_services**:
  		* **local**:
  		* **remote**:
 * **mongodb**:   