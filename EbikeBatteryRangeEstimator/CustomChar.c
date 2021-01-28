unsigned char halfBar[8] = {
        0x03,
        0x03,
        0x03,
        0x03,
        0x03,
        0x03,
        0x03,
        0x03
};

unsigned char cTwo[8] = {
        0x00,
        0x00,
        0x09,
        0x15,
        0x13,
        0x00,
        0x00,
        0x00
};

unsigned char cFour[8] = {
        0x00,
        0x00,
        0x1F,
        0x04,
        0x1C,
        0x00,
        0x00,
        0x00
};

unsigned char cFive[8] = {
    0x00,
    0x00,
    0x16,
    0x15,
    0x1D,
    0x00,
    0x00,
    0x00
};

unsigned char cSix[8] = {
    0x00,
    0x00,
    0x13,
    0x15,
    0x0F,
    0x00,
    0x00,
    0x00
};

unsigned char cSeven[8] = {
    0x00,
    0x00,
    0x18,
    0x14,
    0x13,
    0x00,
    0x00,
    0x00
};

unsigned char cEight[8] = {
    0x00,
    0x00,
    0x1F,
    0x15,
    0x1F,
    0x00,
    0x00,
    0x00
};

unsigned char cNine[8] = {
    0x00,
    0x00,
    0x1E,
    0x15,
    0x1D,
    0x00,
    0x00,
    0x00
};

unsigned char empty[8] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
};

int iterateArray(unsigned char numberPattern[], unsigned char pattern[]) {
    for(int i = 0; i< 9; i++) {
        numberPattern[i] = pattern[i];
    }
    return 0;
}

int getCustomCharPattern(int number, unsigned char pattern[]) {
    switch (number) {
        case 0:
            iterateArray(pattern, halfBar);
            return 0;
        case 2:
            iterateArray(pattern, cTwo);
            return 0;
        case 4:
            iterateArray(pattern, cFour);
            return 0;
        case 5:
            iterateArray(pattern, cFive);
            return 0;
        case 6:
            iterateArray(pattern, cSix);
            return 0;
        case 7:
            iterateArray(pattern, cSeven);
            return 0;
        case 8:
            iterateArray(pattern, cEight);
            return 0;
        case 9:
            iterateArray(pattern, cNine);
            return 0;
        default:
            iterateArray(pattern, empty);
            return 1;
    }
};


// returns lcd character that resemble give char sideways
unsigned char getRemappedChar(char key) {
    switch (key) {
        case 'B':
            return (unsigned char) 0x00;
        case '0':
            return (unsigned char) 'o'; // o
        case '1':
            return (unsigned char) 0x2D; // -
        case '2':
            return (unsigned char) 0x01; //Addres of custom 2 symbol in lcd memory
        case '3':
            return (unsigned char) 0x6D;
        case '4':
            return (unsigned char) 0x02;
        case '5':
            return (unsigned char) 0x03;
        case '6':
            return (unsigned char) 0x04;
        case '7':
            return (unsigned char) 0x05;
        case '8':
            return (unsigned char) 0x06;
        case '9':
            return (unsigned char) 0x07;
        default:
            return (unsigned char) 0xFF;
    }
}

int remapText(char inputText[], unsigned char mappedText[]) {
    for (int i = 0; i < 2; i++) {
        mappedText[i] = getRemappedChar(inputText[i]);
    }
    return 0;
}
