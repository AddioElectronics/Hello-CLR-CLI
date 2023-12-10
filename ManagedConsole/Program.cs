namespace ManagedConsole
{
    internal class Program
    {
        static void Main(string[] args)
        {
            string str = CallNative.GetNativeString();
            Console.WriteLine(str);
        }
    }
}