cmake_minimum_required(VERSION 3.10)
####
# The version number in the previous line may be reduced if needed. I used
# the default in cmake tutorial (oct 2021)
####

project(Ensithreadsvideo)
enable_testing()

set(CMAKE_BUILD_TYPE Debug)

#########
# Gestion des équipes et variante du sujet / Team management and addition
#########
if (EXISTS ${CMAKE_SOURCE_DIR}/AUTHORS)
  message(STATUS "Les auteurs du TP incrits dans AUTHORS sont / AUTHORS are:")
  execute_process (COMMAND cat AUTHORS WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
else()
  message(FATAL_ERROR "** ERREUR / ERROR **: Vous devez mettre vos prénoms, noms et login dans un fichier nommé 'AUTHORS' à la racine du projet / Add an 'AUTHORS' file with all authors' name, firstname and login at the project root; Fichier manquant/missing file: "  ${CMAKE_SOURCE_DIR}/AUTHORS)
endif()

# Trouver la SDL2, ogg, theora et vorbis avec pkgconfig
execute_process(COMMAND bash -c "PKG_CONFIG_PATH=/opt/sdl2-2.0.5/lib/pkgconfig:$PKG_CONFIG_PATH pkg-config --cflags ogg vorbis theora theoradec sdl2" OUTPUT_VARIABLE ALL_PKG_CFLAGS)
execute_process(COMMAND bash -c "PKG_CONFIG_PATH=/opt/sdl2-2.0.5/lib/pkgconfig:$PKG_CONFIG_PATH pkg-config --libs ogg vorbis theora theoradec sdl2" OUTPUT_VARIABLE ALL_PKG_LDFLAGS)
string(STRIP "${ALL_PKG_CFLAGS}" ALLSTRIP_PKG_CFLAGS)
string(STRIP "${ALL_PKG_LDFLAGS}" ALLSTRIP_PKG_LDFLAGS)
# Le Cflags cenTOS Ensimag descends dans SDL2/
# Je le supprime pour être homogène dbns le source
# avec ubuntu et debian
string(REPLACE "-I/opt/sdl2-2.0.5/include/SDL2" "-I/opt/sdl2-2.0.5/include" ALLSTRIP_PKG_CFLAGS "${ALLSTRIP_PKG_CFLAGS}")
message(STATUS "cflags: ${ALLSTRIP_PKG_CFLAGS}")
message(STATUS "ldflags: ${ALLSTRIP_PKG_LDFLAGS}")


# Passer le compilateur CXX au standard C++-23 et ajouter les CFLAGS pour la sdl
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${ALLSTRIP_PKG_CFLAGS} -Wall -Wextra -Werror -fanalyzer")


##
# Si vous utilisez d'autres fichiers pour votre lecteur video il faut
# les ajouter ici
##

add_executable(ensivideo src/ensivideo.cc src/ensitheora.cc src/ensivorbis.cc src/oggstream.cc src/stream_common.cc src/synchro.cc)

target_link_libraries(ensivideo pthread ${ALLSTRIP_PKG_LDFLAGS})


##
# Construction de l'archive
##
string(REPLACE ";" "-" LOGINS_SANS_POINTVIRGULE "${LOGINS}")
set(CPACK_PACKAGE_VERSION_MAJOR "3")
set(CPACK_PACKAGE_VERSION_MINOR "0c++")
set(CPACK_PACKAGE_VERSION_PATCH ${LOGINS_SANS_POINTVIRGULE})
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES
  "~$"
  "\\\\.o$"
  "^${PROJECT_SOURCE_DIR}/build/"
  "^${PROJECT_SOURCE_DIR}/.git/"
)
include(CPack)
