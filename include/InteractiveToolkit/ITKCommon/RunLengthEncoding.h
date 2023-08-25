#pragma once

#include "../common.h"
#include "../EventCore/Callback.h"

namespace ITKCommon
{
    const int RUN_LENGTH_NORMAL = 0;
    const int RUN_LENGTH_BLACK = 1;
    const int RUN_LENGTH_WHITE = 2;

    class RunLengthEncodingBlackWhite
    {

        EventCore::Callback<void(const uint8_t *, size_t)> OnData;

        uint8_t rle_count;
        int state;

    public:
        RunLengthEncodingBlackWhite(const EventCore::Callback<void(const uint8_t *, size_t)> &OnData)
        {
            this->OnData = OnData;
            rle_count = 0;
            state = RUN_LENGTH_NORMAL;
        }

        ~RunLengthEncodingBlackWhite()
        {
            endStream();
        }
        void putByte(uint8_t byte)
        {
            if (byte == 0x00)
            {
                if (state != RUN_LENGTH_BLACK)
                {
                    if (state != RUN_LENGTH_NORMAL)
                    {
                        // flush
                        OnData(&rle_count, 1);
                    }
                    OnData(&byte, 1);
                    state = RUN_LENGTH_BLACK;
                    rle_count = 1;
                }
                else
                {
                    // state = RUN_LENGTH_BLACK
                    if (rle_count == 255)
                    {
                        // flush
                        OnData(&rle_count, 1);
                        uint8_t black = 0x00;
                        OnData(&black, 1);
                        rle_count = 0;
                    }
                    rle_count++;
                }
            }
            else if (byte == 0xff)
            {
                if (state != RUN_LENGTH_WHITE)
                {
                    if (state != RUN_LENGTH_NORMAL)
                    {
                        // flush
                        OnData(&rle_count, 1);
                    }
                    OnData(&byte, 1);
                    state = RUN_LENGTH_WHITE;
                    rle_count = 1;
                }
                else
                {
                    // state = RUN_LENGTH_WHITE
                    if (rle_count == 255)
                    {
                        // flush
                        OnData(&rle_count, 1);
                        uint8_t white = 0xff;
                        OnData(&white, 1);
                        rle_count = 0;
                    }
                    rle_count++;
                }
            }
            else
            {
                if (state != RUN_LENGTH_NORMAL)
                {
                    // flush
                    OnData(&rle_count, 1);
                    state = RUN_LENGTH_NORMAL;
                }
                // RUN_LENGTH_NORMAL case
                OnData(&byte, 1);
            }
        }

        void endStream()
        {
            if (state != RUN_LENGTH_NORMAL)
            {
                // flush
                OnData(&rle_count, 1);
                state = RUN_LENGTH_NORMAL;
            }
        }

        void readFromFile(const char *file)
        {
            FILE *in = fopen(file, "rb");
            if (in)
            {
                while (!feof(in))
                {
                    uint8_t c;
                    size_t readed_size = fread(&c, sizeof(uint8_t), 1, in);
                    putByte(c);
                }
                fclose(in);
                endStream();
            }
        }

        void readFromBuffer(const char *data, size_t size)
        {
            for (size_t i = 0; i < size; i++)
            {
                putByte(data[i]);
            }
            endStream();
        }
    };

    class RunLengthDecodingBlackWhite
    {

        EventCore::Callback<void(const uint8_t *, size_t)> OnData;

        uint8_t original_byte;
        bool rle_will_be_next;

    public:
        RunLengthDecodingBlackWhite(const EventCore::Callback<void(const uint8_t *, size_t)> &OnData)
        {
            this->OnData = OnData;
            original_byte = 0;
            rle_will_be_next = false;
        }

        void putByte(const uint8_t &byte)
        {
            if (rle_will_be_next)
            {
                rle_will_be_next = false;
                for (int i = 0; i < byte; i++)
                    OnData(&original_byte, 1);
            }
            else if (byte == 0x00 || byte == 0xff)
            {
                original_byte = byte;
                rle_will_be_next = true;
            }
            else
            {
                OnData(&byte, 1);
            }
        }

        void readFromFile(const char *file)
        {
            FILE *in = fopen(file, "rb");
            if (in)
            {
                while (!feof(in))
                {
                    uint8_t c;
                    size_t readed_size = fread(&c, sizeof(uint8_t), 1, in);
                    putByte(c);
                }
                fclose(in);
            }
        }

        void readFromBuffer(const char *data, size_t size)
        {
            for (size_t i = 0; i < size; i++)
            {
                putByte(data[i]);
            }
        }
    };

}
