package petrinet;

public class IncompatibleTransition extends Exception {
	private String name;
   
  public IncompatibleTransition(String name) {
  	this.name = name;
  }
   
  public String toString() {
  	return "Place " + name + " doesn't exists in this Net";
  }
}