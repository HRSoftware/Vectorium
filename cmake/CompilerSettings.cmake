function(apply_strict_compiler_settings target_name)
    if(MSVC)
        string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        
        target_compile_options(${target_name} PRIVATE
            /W4				# Max warning level
            /WX				# Treat warning as errors
            /permissive-	# Disable non-conforming code
            /Zc:__cplusplus
            /Zc:preprocessor
            /Zc:externConstexpr
            /Zc:inline
            /Zc:throwingNew
            
            # Disable overly noisy warnings
            /wd4514 /wd4710 /wd4711 /wd4820
            /wd4626 /wd5027  # Assignment operators
            /wd4625 /wd5026  # Copy constructors
            /wd4623          # Default constructor implicitly deleted
        )
        
        target_compile_definitions(${target_name} PRIVATE
            _CRT_SECURE_NO_WARNINGS=1
            WIN32_LEAN_AND_MEAN=1
            NOMINMAX=1
			_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING=1
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target_name} PRIVATE
            -Wall -Wextra -Wpedantic -Werror
        )
    endif()
endfunction()