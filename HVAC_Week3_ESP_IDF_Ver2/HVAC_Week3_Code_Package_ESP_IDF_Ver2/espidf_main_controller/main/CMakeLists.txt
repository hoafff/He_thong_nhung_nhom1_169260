idf_component_register(
    SRCS
        "main.cpp"
        "CommandProtocol.cpp"
        "DHT22Driver.cpp"
        "DisplayUI.cpp"
        "MQ135Driver.cpp"
        "PMS5003Driver.cpp"
        "RuleEngine.cpp"
    INCLUDE_DIRS "."
)

target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++17)
