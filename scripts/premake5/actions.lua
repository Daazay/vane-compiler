-- utility
function remove_dir(path)
    local ok, err = os.rmdir( path )
    if ok then
        printf("Removing %s...", path)
    end
end

function remove_dirs_matched(pattern)
    local matches = os.matchdirs(pattern)

    for i, path in ipairs(matches) do
        remove_dir(path)
    end
end


function remove_file(path)
    local ok, err = os.remove( path )
    if ok then
        printf("Removing %s...", path)
    end
end

function remove_files_matched(pattern)
    local matches = os.matchfiles(pattern)

    for i, path in ipairs(matches) do
        remove_file(path)
    end
end

-- ACTIONS
newaction {
    trigger     = "clean-output",
    description = "Remove the output dir.",
    execute     = function ()
        print("Cleaning the build..")
        remove_dirs_matched(OUTPUT_DIR_PATH)
        print("Done.")
    end
}

newaction {
    trigger     = "clean-vs-related",
    description = "Remove visual studio related files and directories.",
    execute     = function ()
        print("Cleaning visual studio related files and directories..")

        remove_dirs_matched(ROOT_DIR_PATH  .. "/" .. "*.vs")
        remove_files_matched(ROOT_DIR_PATH .. "/" .. "*.sln")
        remove_files_matched(ROOT_DIR_PATH .. "/" .. "**.vcxproj")
        remove_files_matched(ROOT_DIR_PATH .. "/" .. "**.vcxproj.user")
        remove_files_matched(ROOT_DIR_PATH .. "/" .. "**.vcxproj.filters")

        print("Done.")
    end
}