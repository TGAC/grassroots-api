# $Id: Makefile.project_tree_builder.app 191176 2010-05-10 16:12:20Z vakatov $

APP = project_tree_builder

SRC = file_contents \
      msvc_configure \
      msvc_makefile \
      msvc_masterproject_generator \
      msvc_prj_generator \
      msvc_prj_utils \
      msvc_project_context \
      msvc_site \
      msvc_sln_generator \
      proj_builder_app \
      proj_datatool_generated_src \
      proj_item \
      proj_tree \
      proj_tree_builder \
      proj_src_resolver \
      proj_utils \
      resolver \
      msvc_configure_prj_generator \
      proj_projects \
      msvc_dlls_info \
      msvc_prj_files_collector \
      configurable_file \
      ptb_gui \
      ptb_registry \
      mac_prj_generator \
      prj_file_collector

DATATOOL_SRC = msvc71_project property_list


LIB = xutil xncbi xregexp $(PCRE_LIB)

LIBS = $(PCRE_LIBS) $(ORIG_LIBS)

# Build even --without-exe, to avoid breaking --with-flat-makefile
# configurations unable to locate a suitable prebuilt copy.
APP_OR_NULL = app

WATCHERS = gouriano
