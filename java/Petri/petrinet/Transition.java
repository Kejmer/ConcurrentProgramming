package petrinet;

import java.util.Collection;
import java.util.Map;

public class Transition<T> {
	
	private final Map<T, Integer> input;
	private final Map<T, Integer> output;
	private final Collection<T> reset;
	private final Collection<T> inhibitor;

  public Transition(Map<T, Integer> input, Collection<T> reset, Collection<T> inhibitor, Map<T, Integer> output) {
  	this.input = input;
  	this.output = output;
  	this.reset = reset;
  	this.inhibitor = inhibitor;
  }
  
  public Map<T, Integer> getInput() {
  	return input;
  }
  
  public Map<T, Integer> getOutput() {
  	return output;
  }
  
  public Collection<T> getReset() {
  	return reset;
  }
  
  public Collection<T> getInhibitor() {
  	return inhibitor;
  }
  
}