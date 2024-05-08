int main() {
    int ar[5] = {3, 14, 15, 9, 26};
    int max = -1;
    for (int i = 0; i < 5; i++) {
        if (max < ar[i]) max = ar[i];
    }
    int* pt = (int*)0x100;
    *pt = max; 
    return 0;
}
