
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
    yaml_event_t event;
    int done = 0;
    int found = 0;
    CompanyIdentifier company = {0, NULL};

    if (!yaml_parser_initialize(&parser)) {
        fprintf(stderr, "Failed to initialize parser\n");
        exit(EXIT_FAILURE);
    }

    yaml_parser_set_input_file(&parser, file);

    int expecting_key = 0;
    int expecting_value = 0;
    int current_value = 0;

    while (!done) {
        if (!yaml_parser_parse(&parser, &event)) {
            fprintf(stderr, "Parser error %d\n", parser.error);
            exit(EXIT_FAILURE);
        }

        switch (event.type) {
            case YAML_MAPPING_START_EVENT:
                expecting_key = 1;
                break;

            case YAML_SCALAR_EVENT:
                if (expecting_key) {
                    if (strcmp((char *)event.data.scalar.value, "value") == 0) {
                        expecting_key = 0;
                        expecting_value = 1;
                        current_value = 1;
                    } else if (strcmp((char *)event.data.scalar.value, "name") == 0) {
                        expecting_key = 0;
                        expecting_value = 1;
                        current_value = 0;
                    }
                } else if (expecting_value) {
                    if (current_value) {
                        int id = (int)strtol((char *)event.data.scalar.value, NULL, 16);
                        if (id == manufacturer_id) {
                            company.id = id;
                            found = 1;
                        }
                    } else if (found) {
                        company.name = strdup((char *)event.data.scalar.value);
                        done = 1;
                    }
                    expecting_key = 1;
                    expecting_value = 0;
                }
                break;

            case YAML_MAPPING_END_EVENT:
                if (found) {
                    done = 1;
                }
                break;

            case YAML_STREAM_END_EVENT:
                done = 1;
                break;

            default:
                break;
        }

        yaml_event_delete(&event);
    }

    yaml_parser_delete(&parser);
    fclose(file);

    return company;
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
