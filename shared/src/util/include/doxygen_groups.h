/*
** Copyright 2014-2016 The Earlham Institute
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

/**
 *
 *
 * @file
 * @brief
 *
 *
 */


/*
 * Doxygen dosen't currently have a way to specify the order that
 * modules appear on the modules page, they default to appearing in
 * the order in which they were scanned. So to get the ordering that
 * we want, specify them all here in the chosen order.
 */


/* CLIENT */

/**
 * @defgroup client_group Client Library
 * @brief Shared code specific for Clients.
 */


/* DRMAA */

/**
 * @defgroup drmaa_group DRMAA Library
 * @brief Shared code specific for running DRMAA jobs.
 */


/* HANDLERS */

/**
 * @defgroup handler_group Handler Library
 * @brief Utility code for accessing Handlers
 */


/* IRODS */

/**
 * @defgroup irods_group iRODS Library
 * @brief A collection of methods to access parts of an
 * iRODS system.
 */


/* MONGODB */

/**
 * @defgroup mongodb_group MongoDB Library
 * @brief A collection of methods to use to access a MongoDB
 * application.
 */


/* NETWORK */

/**
 * @defgroup network_group Network Library
 * @brief The datatypes and methods for network functionality.
 */


/* PARAMETERS */

/**
 * @defgroup parameters_group Parameters Library
 * @brief The datatypes and methods for dealing
 * with parameters.
 */


/* SERVER */

/**
 * @defgroup server_group Server Library
 * @brief Shared code specific for Servers.
 */


/* SERVICES */

/**
 * @defgroup services_group Services Library
 * @brief Shared code specific for Services.
 */


/* UTILITY */

/**
 * @defgroup utility_group Utility Library
 * @brief General purpose code
 */


/* SERVER IMPLEMENTATIONS */

/* HTTPD */

/**
 * @defgroup httpd_server The Apache HTTPD Module
 * @brief A module allowing Grassroots to run as part of an Apache 2.4.x application.
 */



/* SERVICE IMPLEMENTATIONS */

/**
 * @defgroup blast_service The BLAST Services Module
 * @brief Services for the BLAST suite of tools.
 *
 * This Module performs BLAST searches on both nucleotide and protein
 * databases. It currently offers the functionality given by blastn, blastp
 * and blastx. The BLAST jobs can be run locally or also on a computing
 * cluster using DRMAA.
 */

/**
 * @defgroup ensembl_rest_service The Ensembl Services Module
 * @brief A service that uses the functionality provided by Ensembl Webservices.
 */

/**
 * @defgroup irods_search_service The iRODS Services Module
 * @brief A Service that can perform queries against the iRODS metadata attributes for iRODS collections and data objects.
 */

/**
 * @defgroup pathogenomics_service The Pathogenomics Services Module
 * @brief A Service that can display location- and time-based views of field sample collection.
 *
 * This project is for storing sample data and metadata along with their phenotype and genotype data. As well as allowing
 * text-based searches and results, a web-based map view allows user-friendly access to the information.
 */


/**
 * @defgroup samtools_service The SamTools Services Module
 * @brief Services for the SamTools suite of tools
 *
 * This Module offers some of the functionality available within
 * the <a href="http://www.htslib.org/">SamTools</a> libraries.
 */

/**
 * @defgroup web_service The Web Services Module
 * @brief A reusable Service for using external web services.
 */

/**
 * @defgroup web_search_service The Web Search Services Module
 * @brief A reusable Service for using external web-based search
 * services.
 */


/* HANDLER IMPLEMENTATIONS */

/**
 * @defgroup dropbox_handler_group The Dropbox Handler Module
 * @brief The Handler for accessing files and directories
 * stored on Dropbox accounts.
 */

/**
 * @defgroup file_handler_group The File Handler Module
 * @brief The Handler for accessing files and directories
 * stored on the locally-mounted filesystems.
 */

/**
 * @defgroup http_handler_group The HTTP Handler Module
 * @brief The Handler resources that are accesible over
 * HTTP(S) connections.
 */


/**
 * @defgroup irods_handler_group The iRODS Handler Module
 * @brief The Handler for accessing objects and directories
 * stored on an iRODS system.s
 */


/* CLIENT IMPLEMENTATIONS */

/**
 * @defgroup standalone_client Standalone Client
 * @brief A Client to connect to a Grassroots Server.
 *
 * This Client uses a plugin to choose how to interact with
 * a user. This can be a Graphical User Interface (GUI) or
 * via a command line for example. By default, a Qt-based
 * GUI is used.
 */

#ifndef SHARED_SRC_UTIL_INCLUDE_DOXYGEN_GROUPS_H_
#define SHARED_SRC_UTIL_INCLUDE_DOXYGEN_GROUPS_H_



#endif /* SHARED_SRC_UTIL_INCLUDE_DOXYGEN_GROUPS_H_ */
