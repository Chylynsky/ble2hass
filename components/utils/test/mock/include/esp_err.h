#ifndef MOCK_ESP_EXCEPTION_H
#define MOCK_ESP_EXCEPTION_H

enum esp_err_t : int
{
    ESP_FAIL             = -1,
    ESP_OK               = 0,
    ESP_ERR_INVALID_SIZE = 1,
};

inline const char* esp_err_to_name(esp_err_t err) noexcept
{
    switch (err)
    {
    case ESP_FAIL:
        return "ESP_FAIL";
    case ESP_OK:
        return "ESP_OK";
    default:
        return "OTHER";
    }
}

#endif
