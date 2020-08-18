#pragma once

//
// Copyright (C) 2019 Pharap (@Pharap)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#if defined(ARDUINO)

// Include size_t
#include <stddef.h>

#else

// Include std::size_t
#include <cstddef>

#endif

//
// getSize
//

#if defined(ARDUINO)

template< typename Type, size_t size >
constexpr size_t getSize(const Type (&)[size]) noexcept
{
	return size;
}

#else

template< typename Type, std::size_t size >
constexpr std::size_t getSize(const Type (&)[size]) noexcept
{
	return size;
}

#endif
