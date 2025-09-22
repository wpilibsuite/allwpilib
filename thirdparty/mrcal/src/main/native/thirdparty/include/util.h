// Copyright (c) 2017-2023 California Institute of Technology ("Caltech"). U.S.
// Government sponsorship acknowledged. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include <stdio.h>

#define MSG(fmt, ...) fprintf(stderr, "%s(%d): " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
