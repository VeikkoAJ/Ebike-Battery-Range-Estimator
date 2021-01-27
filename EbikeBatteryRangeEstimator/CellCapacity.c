// values store as [mV, mA] pairs
int lookUpTable[99][2] = {
  {2703, 2600},
  {2753, 2574},
  {2824, 2548},
  {2970, 2522},
  {3224, 2496},
  {3316, 2470},
  {3373, 2444},
  {3417, 2418},
  {3453, 2392},
  {3478, 2366},
  {3496, 2340},
  {3511, 2314},
  {3523, 2287},
  {3534, 2260},
  {3544, 2234},
  {3552, 2207},
  {3560, 2181},
  {3567, 2155},
  {3573, 2129},
  {3580, 2101},
  {3586, 2074},
  {3591, 2047},
  {3596, 2021},
  {3601, 1995},
  {3606, 1969},
  {3610, 1943},
  {3615, 1915},
  {3619, 1889},
  {3623, 1863},
  {3627, 1835},
  {3631, 1806},
  {3634, 1777},
  {3636, 1751},
  {3639, 1721},
  {3642, 1692},
  {3645, 1663},
  {3648, 1636},
  {3651, 1609},
  {3654, 1583},
  {3658, 1554},
  {3661, 1526},
  {3665, 1499},
  {3668, 1473},
  {3672, 1447},
  {3676, 1420},
  {3680, 1393},
  {3684, 1367},
  {3688, 1340},
  {3693, 1313},
  {3698, 1285},
  {3703, 1258},
  {3708, 1232},
  {3713, 1205},
  {3719, 1177},
  {3725, 1151},
  {3731, 1125},
  {3738, 1099},
  {3745, 1072},
  {3752, 1046},
  {3759, 1020},
  {3767, 993},
  {3775, 967},
  {3784, 941},
  {3793, 914},
  {3802, 887},
  {3812, 861},
  {3821, 835},
  {3831, 808},
  {3841, 782},
  {3852, 756},
  {3862, 730},
  {3872, 704},
  {3883, 678},
  {3893, 652},
  {3904, 626},
  {3914, 600},
  {3925, 574},
  {3935, 548},
  {3946, 521},
  {3957, 495},
  {3968, 468},
  {3979, 441},
  {3990, 414},
  {4001, 387},
  {4012, 361},
  {4022, 335},
  {4033, 309},
  {4044, 283},
  {4055, 257},
  {4066, 230},
  {4077, 204},
  {4088, 178},
  {4100, 152},
  {4112, 126},
  {4125, 100},
  {4138, 74},
  {4154, 48},
  {4172, 22},
  {4200,3},
};


int getCapacity(int voltage) {
    for (int i = 0; i < 99; i++) {
        if (voltage < lookUpTable[i][0]) {
            if (i > 1) {
                return ((lookUpTable[i][1] + lookUpTable[i - 1][1]) / 2);
            }
        return lookUpTable[i][1];
    }
  }
  return 0;
};
