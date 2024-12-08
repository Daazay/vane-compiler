project "testbed"
    kind            "ConsoleApp"
    language        "C"
    cdialect        "C17"
    systemversion   "latest"
    warnings        "extra"
    -- 
    location        (".")
    targetdir       (OUTPUT_BIN_DIR_PATH)
    objdir          (OUTPUT_OBJ_DIR_PATH)
    -- 
    files {
        "src/**.h",
        "src/**.c",
    }
    -- 
    includedirs {
        PROJECTS_DIR_PATH .. "vanec/include",
    }
    -- 
    links {
        "vanec"
    }
    -- 
    debugargs { 
        "--chunk_cap",
        "32",
        "ast",
        ("\"" .. EXAMPLES_DIR_PATH .. "armstrong_numbers.vn" .. "\""),
        ("\"" .. EXAMPLES_DIR_PATH .. "fizz_buzz.vn" .. "\""),
        ("\"" .. EXAMPLES_DIR_PATH .. "gcd.vn" .. "\"")
    }