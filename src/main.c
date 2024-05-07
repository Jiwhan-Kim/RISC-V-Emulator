int main() {
    char ar[5] = {3, 14, 15, 9, 26};
    char max = -1;
    for (char i = 0; i < 5; i++) {
        if (max < ar[i]) max = ar[i];
    }
    return 0;
}
