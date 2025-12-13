#ifndef IMPLEMENT_MEMMOVE_H
#define IMPLEMENT_MEMMOVE_H

namespace impl_memmove
{
	char new_dest[3];
	char buffer[] = "ABCDE";

	void* my_memmove(void* dst, const void* src, size_t n)
	{
		unsigned char* d = static_cast<unsigned char*>(dst);
		const unsigned char* s = static_cast<const unsigned char*>(src);

		// your pointer-only logic here
		if (d > s)
		{
			std::cout << "Backward copy" << std::endl;

			for (int i = n - 1; i >= 0; --i)
			{
				*(d + i) = *(s + i);
			}
		}
		else
		{
			std::cout << "Forward copy" << std::endl;

			for (int i = 0; i < n; ++i)
			{
				*(d + i) = *(s + i);
			}
		}

		return dst;
	}
}


#endif // !IMPLEMENT_MEMMOVE_H
