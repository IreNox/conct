
namespace conct
{
	bool runSerialTest();

	bool run()
	{
		bool result = true;
		result &= runSerialTest();

		return result;
	}
}

int main()
{
    return conct::run() ? 0 : 1;
}
