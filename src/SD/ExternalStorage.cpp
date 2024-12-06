#include "ExternalStorage.h"
#include "Config.h"

#include <dirent.h>
#include "esp_vfs_fat.h"

namespace PrintServer
{
    static ExternalStorage* external_storage = nullptr;

    void ExternalStorage::start_reading_file(const char *path)
    {
        reserved_reading = true;
        char buffer[256 + strlen((SD_MOUNT_POINT"/"))];
        buffer[0] = '\0';

        strcpy(buffer, (SD_MOUNT_POINT"/"));
        strcat(buffer, path);

        open_file_read(buffer);
    }
    
    void ExternalStorage::stop_reading_file(const char *path)
    {
        reserved_reading = false;
    }

    //returns true if a line was taken from the file
    int ExternalStorage::get_next_line(char* line_buffer) 
    {
        if (current_open_file == nullptr) 
        {
            return 0;
        }
        while (fgets(line_buffer, MAX_GCODE_LENGTH, current_open_file) != nullptr) 
        {
            // Strip the newline character if present
            //line_buffer[strcspn(line_buffer, "\n")] = 0;

            // Skip empty lines
            if (line_buffer[0] == '\0') 
            {
                continue; // Go to the next line
            }
            
            else if (line_buffer[0] == '\n')
            {
                continue;
            }

            // Skip lines starting with a semicolon (comment line)
            if (line_buffer[0] == ';') 
            {
                continue; // Go to the next line
            }

            // Find if there's a semicolon in the line (indicating a comment)
            char* comment_pos = strchr(line_buffer, ';');
            if (comment_pos != nullptr) 
            {
                // Null-terminate the line before the semicolon (ignore comment part)
                *comment_pos = '\n';
                return comment_pos - line_buffer + 1;
            }

            //line_buffer[MAX_GCODE_LENGTH] = '\n';

            return strlen(line_buffer); // Valid line, return true
        }
        return 0;
    }

    void list_files_in_directory(const char *path) 
    {
        DIR *dir = opendir(path);
        if (dir == NULL) 
        {
            ESP_LOGI("SD","Failed to open directory: %s\n", path);
            return;
        }

        ESP_LOGI("SD","Files in directory '%s':\n", path);
        struct dirent *entry;
        int file_index = 0;
        while ((entry = readdir(dir)) != NULL) 
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, SAVE_FILE) == 0) 
            {
                continue;
            }

            char full_path[256];
            snprintf(full_path, sizeof(full_path) + 1, "%s/%s", path, (char*)(entry->d_name));

            struct stat entry_stat;
            if (stat(full_path, &entry_stat) == 0) 
            {
                if (S_ISREG(entry_stat.st_mode)) 
                {
                    server_data_struct.add_file(entry->d_name);
                } 
                else if (S_ISDIR(entry_stat.st_mode)) 
                {
                    ESP_LOGI("SD","  Directory: %s\n", entry->d_name);
                }
            } 
            else 
            {
                ESP_LOGI("SD","  Failed to stat: %s\n", full_path);
            }
        }
        //server_data_struct.sd_file_count = file_index;
        closedir(dir);
    }

    void save_struct_to_file(const char *filename, saved_server_data_t *data) {
        FILE *file = fopen(filename, "wb");
        if (file == NULL) 
        {
            printf("Failed to open file for writing\n");
            return;
        }

        size_t written = fwrite(data, sizeof(saved_server_data_t), 1, file);
        if (written != 1) 
        {
            printf("Failed to write struct to file\n");
        } 
        else 
        {
            printf("Struct saved successfully\n");
        }

        fclose(file);
    }
    
    void read_struct_from_file(const char *filename, saved_server_data_t *data) 
    {
        FILE *file = fopen(filename, "rb");
        if (file == NULL) 
        {
            printf("Failed to open file for reading\n");
            return;
        }

        size_t read = fread(data, sizeof(saved_server_data_t), 1, file);
        if (read != 1) 
        {
            printf("Failed to read struct from file\n");
        } 
        else 
        {
            printf("Struct read successfully\n");
        }

        fclose(file);
    }
    
    ExternalStorage::ExternalStorage()
    {
        esp_err_t ret;

        server_data_struct.sd_size_gb = sd_device.get_capacity();
        server_data_struct.sd_speed_mhz = sd_device.get_freq_mhz();
        server_data_struct.sd_card_detected = 1;

        list_files_in_directory(SD_MOUNT_POINT);

        read_struct_from_file("/sdcard/"SAVE_FILE, &saved_server_data_struct);
    }

    ExternalStorage::~ExternalStorage()
    {

    }

    void ExternalStorage::save_data()
    {
        save_struct_to_file("/sdcard/server_data.bin", &saved_server_data_struct);
    }

    bool ExternalStorage::init()
    {
        external_storage = new ExternalStorage();
        return 1;
    }

    void ExternalStorage::shutdown()
    {

    }

    ExternalStorage &ExternalStorage::get_instance()
    {
        return *external_storage;
    }

    void ExternalStorage::open_file(const char *path)
    {
        ESP_LOGI(DEBUG_NAME, "Opening file %s", path);
        server_data_struct.add_file(path + strlen(SD_MOUNT_POINT"/"));
        current_open_file = fopen(path, "w");
        if (current_open_file == NULL) 
        {
            ESP_LOGE(DEBUG_NAME, "Failed to open file for writing");
        }
    }
   void ExternalStorage::open_file_read(const char *path)
    {
        ESP_LOGI(DEBUG_NAME, "Opening file %s", path);
        current_open_file = fopen(path, "r");
        if (current_open_file == NULL) 
        {
            ESP_LOGE(DEBUG_NAME, "Failed to open file for writing");
        }
    }

    void ExternalStorage::close_file()
    {
        fclose(current_open_file);
        current_open_file = nullptr;
    }

    void ExternalStorage::delete_file(const char *filename)
    {
        char full_path[256];
        snprintf(full_path, sizeof(full_path) + 1, "%s/%s", SD_MOUNT_POINT, (char*)(filename));
        ESP_LOGI(DEBUG_NAME, "Deleting file: %s", full_path);

        remove(full_path);

        server_data_struct.remove_file(filename);
    }

    //Returns chars written
    int ExternalStorage::write_to_open_file(const char *data, int chars)
    {
        int written_chars = 0;
        written_chars = fwrite(data, sizeof(char), chars, current_open_file);
        
        return written_chars;
    }

    esp_err_t ExternalStorage::print_file(const char *path)
    {
        ESP_LOGI(DEBUG_NAME, "Reading file %s", path);
        FILE *f = fopen(path, "r");
        if (f == NULL) 
        {
            ESP_LOGE(DEBUG_NAME, "Failed to open file for reading");
            return ESP_FAIL;
        }
        char line[SD_WRITE_MAX_CHAR_SIZE];
        fgets(line, sizeof(line), f);
        fclose(f);

        // strip newline
        //char *pos = strchr(line, '\n');
        //if (pos) 
        //{
        //    *pos = '\0';
        //}
        ESP_LOGI(DEBUG_NAME, "Read from file: '%s'", line);

        return ESP_OK;
    }

    void ExternalStorage::unmount() 
    {

    };
}