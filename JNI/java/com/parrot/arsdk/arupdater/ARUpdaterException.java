
package com.parrot.arsdk.arupdater;


public class ARUpdaterException extends Exception
{	
    private ARUPDATER_ERROR_ENUM error;
    
    /**
     * ARUpdaterException constructor
     * @return void
     */
    public ARUpdaterException()
    {
        error = ARUPDATER_ERROR_ENUM.ARUPDATER_ERROR;
    }
    
    /**
     * ARUpdaterException constructor
     * @param error ARUPDATER_ERROR_ENUM error code
     * @return void
     */
    public ARUpdaterException(ARUPDATER_ERROR_ENUM error) 
    {
        this.error = error;
    }
    
    /**
     * ARUpdaterException constructor
     * @param error int error code
     * @return void
     */
    public ARUpdaterException(int error) 
    {
        this.error = ARUPDATER_ERROR_ENUM.getFromValue(error);
    }
    
    /**
     * Gets ARUpdater ERROR code
     * @return {@link ARUPDATER_ERROR_ENUM} error code
     */
    public ARUPDATER_ERROR_ENUM getError()
    {
        return error;
    }
    
    /**
     * Sets ARUpdater ERROR code
     * @param error {@link ARUPDATER_ERROR_ENUM} error code     
     * @return void
     */
    public void setError(ARUPDATER_ERROR_ENUM error)
    {
        this.error = error;
    }
    
    /**
     * Gets ARUpdaterException string representation
     * @return String Exception representation
     */
    public String toString ()
    {
        String str;
        
        if (null != error)
        {
            str = "ARUpdaterException [" + error.toString() + "]";
        }
        else
        {
            str = super.toString();
        }
        
        return str;
    }
}

