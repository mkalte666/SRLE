#ifndef SRLE_H
#define SRLE_H

/**
 * \author Malte Kieﬂling 
 * \copyright Copyright 2017 Malte Kieﬂling. All rights reserved. 
 * \licence This project is released under the MIT Licence. 
 *          See LICENCE in the project root for details. 
 */

#include <cstdint>

/**
 * Classes for (Stream) Run Length En- and Decoding
 */
namespace SRLE {
    /// Default control character 
    static const uint8_t DEFAULT_CTRL = 0xFA;
    /**
      * \brief Function pointer used for the write callback
      * Parameter is a uint8_t - the byte to read/write
    */
    using WriteFuncPtr = void(*)(uint8_t);

    /**
      * \brief Stream Run Length Encoding Encoder 
      * Does run-length encoding on an input stream. 
      * The Control Character can be selected with the template argument. 
      * 
      * Sequences smaller than 4 bytes (and if they are not equal the ctrl character) 
      * are ignored. (written as-is).
      * Sequences bigger than that become 3 bytes long: 
      * <ctrl> <byte> <count> 
      * Example: "ABCDEFAAAAAAAA", ctrl is 0xFA
      * Becomes: "ABCDEF<0xFA>A<0x08>
      *
      * \note Call writeSequence() after you are done to make sure no bytes 
      * are forgotten.
      * \param ctrl The Control Character for the run-length encoding
    */
    template<uint8_t ctrl = DEFAULT_CTRL>
    class Encoder
    {
    public:
        /**
          * \param writeFunc pointer to the function that recieves the encoded bytes 
        */
        explicit Encoder(WriteFuncPtr writeFunc)
            : writeFunc(writeFunc)
            , lastByte(0)
            , counter(0)
        {}

        /// Prohibit copy. This should not be copied ever
        Encoder(const Encoder& that) = delete;

        /**
            \brief Process a whole bunch of bytes
            \param s pointer to the data
            \param l length of the data 
            \note Call writeSequence to finish up after encoding chunks of data to make sure nothing is lost
        */
        void write(void* s, size_t l)
        {
            uint8_t* b = (uint8_t*)s;
            for (size_t i = 0; i < l; i++) {
                putb(b[i]);
            }
        }

        /**
         * \brief puts a whole string 
         */
        void puts(const char* s)
        {
            while(*s) {
                putb((uint8_t)*s);
                s++;
            }
        }

        /**
         * \brief Process a byte
         * \note Call writeSequence to finish up after encoding chunks of data to make sure nothing is lost
         */
        inline void putb(uint8_t b)
        {
            // nothing written yet
            if (counter == 0) {
                lastByte = b;
                counter = 1;
                return;
            }
            // ongoing sequence
            if (b == lastByte) {
                counter++;
                // avoid overflow
                if (counter >= 255) {
                    writeSequence();
                }
                return;
            }
            // we do not match 
            writeSequence();
            counter = 1; // we store b 
            lastByte = b;
        }

        /**
         * \brief Finishes up the current sequence and writes it to the output function
         */
        void writeSequence()
        {
            if (counter == 0) {
                return;
            }

            if (counter > 3 || ctrl == lastByte) {
                writeFunc(ctrl);
                writeFunc(lastByte);
                writeFunc(counter);
            }
            else {
                for (uint8_t i = 0; i < counter; i++) {
                    writeFunc(lastByte);
                }
            }

            counter = 0;
        }

    private:
        WriteFuncPtr writeFunc; ///< function used for writing
        uint8_t lastByte; ///< last byte
        uint8_t counter; ///< counter of the sequence
    };

    /**
     * \brief Streaming Run Lenght Encoding Decoder 
     * \see SRLE::Encoder 
     * Decodes a data stream. 
     */
    template<uint8_t ctrl = DEFAULT_CTRL>
    class Decoder
    {
    public:
        /**
        * \param writeFunc pointer to the function that recieves the encoded bytes
        */
        explicit Decoder(WriteFuncPtr writeFunc)
            : writeFunc(writeFunc)
            , gotCtrl(false)
            , gotByte(false)
            , lastByte(0)
        {}

        /// Prohibit copy. This should not be copied ever
        Decoder(const Decoder& that) = delete;

        /**
        \brief Process a whole bunch of bytes
        \param s pointer to the data
        \param l length of the data
        */
        void write(void* s, size_t l)
        {
            uint8_t* b = (uint8_t*)s;
            for (size_t i = 0; i < l; i++) {
                putb(b[i]);
            }
        }

        /**
         * \brief Decoed next byte 
         * \note This will put bigger into the writeFunc (up to 255 bytes)
         */
        void putb(uint8_t b)
        {
            if (!gotCtrl) {
                if (b != ctrl) {
                    writeFunc(b);
                    return;
                }
                gotCtrl = true;
                return;
            }

            if (!gotByte) {
                lastByte = b;
                gotByte = true;
                return;
            }

            for (uint8_t i = 0; i < b; i++) {
                writeFunc(lastByte);
            }
            gotByte = gotCtrl = false;
        }

    private:
        WriteFuncPtr writeFunc; ///< function to call
        bool gotCtrl; ///< if we got a control byte
        bool gotByte; ///< if we got the data byte
        int8_t lastByte; ///< the data byte in the sequence
    };

}; // namespace SRLE

#endif //SRLE_H