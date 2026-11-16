#include <stdio.h>
#include <string.h>

// Standard CRC-8 polynomial: x^8 + x^2 + x + 1 (0x07)
#define POLYNOMIAL 0x07

unsigned char compute_crc8(const unsigned char *data, size_t length)
{
    unsigned char crc = 0x00; // Initial remainder (commonly 0x00 or 0xFF)

    for (size_t i = 0; i < length; i++)
    {
        // Bring the next byte of data into the CRC register
        crc ^= data[i];

        // Process each of the 8 bits in the current register byte
        for (int bit = 0; bit < 8; bit++)
        {
            // Check if the Most Significant Bit (MSB) is 1
            if (crc & 0x80)
            {
                // If MSB is 1, shift left and XOR with the polynomial divisor
                crc = (crc << 1) ^ POLYNOMIAL;
            }
            else
            {
                // If MSB is 0, simply shift left
                crc = (crc << 1);
            }
        }
    }
    return crc;
}

int main()
{
    // The exact string sent via netcat in the previous labs
    const char *message = "HELLO";

    unsigned char crc_result = compute_crc8((const unsigned char *)message, strlen(message));

    printf("Message:    \"%s\"\n", message);
    printf("Hex representation: ");
    for (size_t i = 0; i < strlen(message); i++)
    {
        printf("%02X ", (unsigned char)message[i]);
    }

    printf("\nComputed CRC-8 Checksum: 0x%02X\n", crc_result);

    return 0;
}
