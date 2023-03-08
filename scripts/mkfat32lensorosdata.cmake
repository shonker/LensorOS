# Run like `cmake -P mkfat32lensorosdata.cmake`

set(REPO_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
set(IMAGE_DIR ${REPO_DIR}/bin)
set(SCRIPTS_DIR ${REPO_DIR}/scripts)

# Boot media generation: raw FAT32 images.
# TODO: Can also use truncate in place of dd
find_program( DD_PROGRAM dd )
find_program( MTOOLS_PROGRAM mtools )
if(NOT DD_PROGRAM OR NOT MTOOLS_PROGRAM)
  message(NOTICE "[31;5mWARN: MISSING PROGRAM!  Could not find `dd` and `mtools`, cannot create LensorOS Data partition. See dependencies in README.[m")
endif()

file(
  COPY_FILE
  ${REPO_DIR}/kernel/res/dfltfont.psf
  ${IMAGE_DIR}/dfltfont.psf
)

function(mcopy_file filename_to_copy directory)
  if (NOT EXISTS ${filename_to_copy})
    message(FATAL_ERROR "File at " ${filename_to_copy} " MUST exist to generate LensorOS Data partition")
  endif()
  execute_process(COMMAND mcopy -i ${IMAGE_DIR}/LensorOSData.img ${filename_to_copy} ::${directory})
endfunction()

message(STATUS "Generating FAT32 LensorOS Data partition...")
file(MAKE_DIRECTORY ${IMAGE_DIR})
execute_process(COMMAND ${DD_PROGRAM} if=/dev/zero of=${IMAGE_DIR}/LensorOSData.img count=93750)
execute_process(COMMAND mformat -i ${IMAGE_DIR}/LensorOSData.img -F -v "LensorOS" ::)
execute_process(COMMAND mmd -i ${IMAGE_DIR}/LensorOSData.img ::/bin)
execute_process(COMMAND mmd -i ${IMAGE_DIR}/LensorOSData.img ::/res)
execute_process(COMMAND mmd -i ${IMAGE_DIR}/LensorOSData.img ::/res/fonts)
execute_process(COMMAND mmd -i ${IMAGE_DIR}/LensorOSData.img ::/res/fonts/psf1)
mcopy_file(${IMAGE_DIR}/dfltfont.psf /res/fonts/psf1)
mcopy_file(${REPO_DIR}/user/bin/blazeit /bin)
mcopy_file(${REPO_DIR}/user/bin/stdout /bin)
mcopy_file(${REPO_DIR}/user/bin/pwd /bin)
# TODO get rid of following lines once directory traversal works in FAT driver...
mcopy_file(${IMAGE_DIR}/dfltfont.psf "")
mcopy_file(${REPO_DIR}/user/bin/stdout "")
mcopy_file(${REPO_DIR}/user/bin/blazeit "")
mcopy_file(${REPO_DIR}/user/bin/pwd "")
