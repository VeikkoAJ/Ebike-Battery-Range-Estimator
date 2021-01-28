char CustomTwo[8] = {
        0x00,
        0x00,
        0x09,
        0x15,
        0x13,
        0x00,
        0x00,
        0x00
};


// returns lcd character that resemble give char sideways
char getRemappedChar(char key) {
    switch (key) {
        case '0':
            return 'o';
        case '1':
            return 0x2D;
        case '3':
            return 0x6D:
        default
            return 0x00;
    }
}