
#include "wirelescanner.h"
#include <yaml.h>

typedef struct {
    int id;
    char *name;
} CompanyIdentifier;
CompanyIdentifier get_company_identifier(int manufacturer_id) {
    FILE *file = fopen("dependencies/public/assigned_numbers/company_identifiers/company_identifiers.yaml", "r");
    if (!file) {
        fprintf(stderr, "Failed to open YAML file\n");
        exit(EXIT_FAILURE);
    }

    yaml_parser_t parser;
    yaml_token_t token;

    /* Initialize parser */
    if (!yaml_parser_initialize(&parser)) {
        fprintf(stderr, "Failed to initialize parser\n");
        exit(EXIT_FAILURE);
    }

    yaml_parser_set_input_file(&parser, file);

    /* Parsing loop */
    int found = 0;
    while (!found) {
        /* Get the next token */
        if (!yaml_parser_scan(&parser, &token)) {
            fprintf(stderr, "Parser error %d\n", parser.error);
            exit(EXIT_FAILURE);
        }

        /* Check for end of file */
        if (token.type == YAML_STREAM_END_TOKEN) {
            break;
        }

        /* Check for mapping start */
        if (token.type == YAML_MAPPING_START_EVENT) {
            /* Loop over mapping key-value pairs */
            while (yaml_parser_scan(&parser, &token)) {
                if (token.type == YAML_SCALAR_TOKEN) {
                    char *key = strdup((char *)token.data.scalar.value);
                    yaml_token_delete(&token);

                    if (!yaml_parser_scan(&parser, &token)) {
                        fprintf(stderr, "Parser error %d\n", parser.error);
                        exit(EXIT_FAILURE);
                    }

                    if (token.type == YAML_SCALAR_TOKEN) {
                        char *value = strdup((char *)token.data.scalar.value);
                        yaml_token_delete(&token);

                        if (strcmp(key, "companyIdentifier") == 0) {
                            int id = atoi(value);
                            if (id == manufacturer_id) {
                                CompanyIdentifier company;
                                company.id = id;
                                company.name = strdup((char *)value);
                                found = 1;
                                free(key);
                                free(value);
                                yaml_parser_delete(&parser);
                                fclose(file);
                                return company;
                            }
                        }

                        free(value);
                    }

                    free(key);
                }
            }
        }
    }

    yaml_parser_delete(&parser);
    fclose(file);
    CompanyIdentifier empty_company = {0, NULL};
    return empty_company;
}

void print_device_info(const char *mac, int rssi, const uint8_t *data, size_t data_len) {
    size_t index;
    uint8_t field_len;
    uint8_t field_type;
    int manufacturer_id;

    printf("Device: %s - RSSI: %d\n", mac, rssi);
    printf("Advertising Data: ");
    for (size_t i = 0; i < data_len; i++) {
        printf("%02x ", data[i]);
    }
    printf("\n");
    index = 0;
    while (index < data_len) {
        field_len = data[index];
        if (field_len == 0 || index + field_len >= data_len)
            break;
        field_type = data[index + 1];
        switch (field_type) {
            case 0x09: // Complete Local Name
            case 0x08: // Shortened Local Name
                printf("Device Name: %.*s\n", field_len - 1, &data[index + 2]);
                break;
            case 0xFF: // Manufacturer Specific Data
                if (field_len >= 3) {
                    manufacturer_id = data[index + 2] | (data[index + 3] << 8);
                    printf("Manufacturer ID: %04x\n", manufacturer_id);
                    CompanyIdentifier company = get_company_identifier(manufacturer_id);
                    printf("Manufacturer Name: %s\n", company.name ? company.name : "Unknown");
                    // You can print other company identifier information here
                    free(company.name);
                }
                break;
            // Handle other field types if needed
        }
        index += field_len + 1;
    }
    printf("\n");
}
