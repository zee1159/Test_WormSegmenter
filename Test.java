public class Test{
    public native String run(String path, String fname);

    static{
        System.load("/home/zkhavas/Documents/Work/Code/Test_WormSegmenter/Test.so");
    }

    public static void main(String[] args){
        String fname = "0000000.jpg";
		String output = null;
        String path = "/home/zkhavas/Documents/Work/Code/Test_WormSegmenter/data/";
        Test test = new Test();
        System.out.println("Running cpp file...\n");
        output = test.run(path, fname);
        System.out.println("Executed cpp file... " + output);
    }
}
