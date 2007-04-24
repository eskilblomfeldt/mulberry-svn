/******************************************************************************
 testhash.cc

	Test hash functions.

	Copyright � 1997 by Dustin Laurence.  All rights reserved.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include <jHashFunctions.h>
#include <iomanip>
#include <jAssert.h>

	JHashValue DualHashArg[] =
		{
		12345678,
		0x00,
		0xFFFFFFFF,
		0xabcdef00, // Test how much close hash values are broken up
		0xabcdef01,
		0xabcdef02,
		0xabcdef03,
		0xabcdef04,
		0xabcdef05,
		0xabcdef06,
		0xabcdef07,
		0xabcdef08,
		0xabcdef09,
		0xabcdef0a,
		0xabcdef0b,
		0xabcdef0c,
		0xabcdef0d,
		0xabcdef0e,
		0xabcdef0f
		};

	JDualHashValue DualHashResult[] =
		{
		-3,
		-7,
		71,
		-117,
		63,
		-15,
		-91,
		87,
		11,
		-67,
		113,
		35,
		-41,
		-119,
		61,
		-17,
		-93,
		85,
		9,
		0x00 // Sentinel, zero is never returned
		};

	JCharacter* HashStringArg[] =
		{
		"l",
		"m",
		"n",

		"AAAAAAAA",
		"AAAAAAAB",
		"AAAAAAA",
		"AAAAAAAAA",

		"ahashkey",
		"bhashkey",
		"chashkey",

		"Cachalot",
		"Cachalos",
		"Cachalor",

		"Fourscore and seven years ago, our forefathers brought forth on this "
		"continent a new nation, dedicated to the proposition that all men are "
		"created equal.  We are now engaged in a great struggle to determine "
		"whether government of the people, by the people, and for the people shall "
		"long endure.",

		"\xA3\x32\x01\x87\x16",
		"\xA3\x32\x02\x87\x16",

		NULL
		};

	JHashValue Hash7BitResult[] =
		{
		0x472600db,
		0x473f66e8,
		0x4758ccf5,

		0x7a1a8d5f,
		0x7a33f36c,
		0xf651292c,
		0xdb0342ac,

		0x27b7ab4f,
		0x319f32c2,
		0x3b86ba35,

		0xa26de3f6,
		0xa2547de9,
		0xa23b17dc,

		0xe1a50679,

		0x6fd5baec,
		0xb00c53f9
		};

	JHashValue Hash8BitResult[] =
		{
		0x472600db,
		0x473f66e8,
		0x4758ccf5,

		0xb2cf594f,
		0xb2e8bf5c,
		0xd6e325d8,
		0x5365cefc,

		0x6b23443,
		0x93ecf9fa,
		0x2127bfb1,

		0x13c40c06,
		0x13aaa5f9,
		0x13913fec,

		0xf1197dd5,

		0x760dc870,
		0xe0993db5
		};

/******************************************************************************
 main

 *****************************************************************************/

int
main()
{
	cout << "Beginning hash function test.  No news is good news" << endl;

// Test JDualHash
	cout << std::setbase(10);
	JIndex i=0;
	while (DualHashResult[i] != 0)
		{
		JDualHashValue result = JDualHash(DualHashArg[i]);
		if (DualHashResult[i] != result)
			{
			cout << "   JDualHash(" << DualHashArg[i] << ") returned "
			     << result << " instead of " << DualHashResult[i] << endl;
			}
		i++;
		}

// Test JHash7Bit
	cout << std::setbase(16);
	i=0;
	while (HashStringArg[i] != NULL)
		{
		JHashValue result = JHash7Bit(HashStringArg[i]);
		if (Hash7BitResult[i] != result)
			{
			cout << "   JHash7Bit(\"" << HashStringArg[i] << "\") returned "
			     << result << " instead of " << Hash7BitResult[i] << endl;
			}
		i++;
		}

// Test JHash8Bit
	cout << std::setbase(16);
	i=0;
	while (HashStringArg[i] != NULL)
		{
		JHashValue result = JHash8Bit(HashStringArg[i]);
		if (Hash8BitResult[i] != result)
			{
			cout << "   JHash8Bit(\"" << HashStringArg[i] << "\") returned "
			     << result << " instead of " << Hash8BitResult[i] << endl;
			}
		i++;
		}

	cout << "Finished hash function test.  If nothing printed out, it passed" << endl;

	return 0;
}
